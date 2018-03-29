startup<-read.table("/home/wyatt/Documents/Arboreal/Source/Data/startup_time.txt", header = TRUE)


startup<-startup[startup$files > 0,]

plot(log(time)~files, data = startup)
plot(log(time)~tags, data = startup)


fit<-lm(log(time)~ files + tags + log(files) + log(tags) + I(files^2), data = startup)
summary(fit)
plot(fit)

plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Tags", main="Tags Residual Plot")
