startup<-read.table("/home/wyatt/Documents/Arboreal/Source/DataDone/create_tag_time.txt", header = TRUE)


plot(log(time)~files, data = startup)
plot(log(time)~tags, data = startup)


fit<-lm(log(time)~files, data = startup)
summary(fit)
plot(fit)

plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Tags", main="Tags Residual Plot")
